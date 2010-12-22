<?php

/**
 * This is our model file. In this file we place all logic.
 * You could place html excentric files in view.blog_simple.inc
 * (which is always included when module is loaded)
 *
 * Or you could make a directory like modules/blog_simple/views
 *
 * This is up to you.
 *
 * In order to make this tutorial simple we place almost anything in
 * the model file.
 */

// We include mysqlForm.php which is easy to use for creating forms connected
// to a database. You don't need to use this class when you make your
// modules. In fact you could use the Zend Form, Your own form class, or just write
// the form as HTML and place it somewhere in the module dir.
//
// Though: For the uniform look and feel of a form, using these class makes it
// quite easy to manipulate looks of the forms in the template.

include_once "mySqlForm.php";

class blogSimple {

    /**
     * A simple static variable holding error codes and messages.
     * @var array   array for holding error codes
     */
    public static $errors = array();


    /**
     * method for getting a filtered entry id.
     * @return int  returns filtered int
     */
    public static function getEntryId (){

        // We will use the uri class for getting info about the
        // URI. In the case the first fragment [0] is the module name
        // the next [1] is the controller name, and the third[2] is
        // an ID identifying the id of the blog post when we will read,
        // update or delete a post.

        return filter_var(uri::$fragments[2], FILTER_VALIDATE_INT);
    }

    /**
     * function for creating a form for insert, update and deleting
     * blog entries. 
     *
     * @param string    method (update, delete or insert)
     * @param int       id (if delete or update)
     * @param array     array with values if we load the form for
     *                  an update operation
     */
    public static function blogForm($method, $id = null, $values = array()){
        
        // This is the forms fields captions
        // As you can see we make our module language aware,
        // so we can easily translate the module strings into other languages
        $fields_captions =
            array(
                // lang::translate translates into language
                // set in config/config.ini
                'title' => lang::translate('blog_simple_title'),
                'content' => lang::translate('blog_simple_content'),
        );

        // which fields do we use of the db table
        $fields = array('id', 'title', 'content');

        // Init the form object with fields and specify which
        // table to use for our form. 'blog_simple' was the
        // name of the blog_simple db table. Fields is which
        // fields to make input fields for. And values is
        // only used if form is used for doing update
        $tf = new mySqlForm('blog_simple', $fields, $values);

        // we also set the labels for the form as specified above.
        $tf->setLabels($fields_captions);

        
        if (isset($id)){
            // if an id is set if update or delete
            $tf->setMethod($method, $id);
            if ($method == 'delete'){
                // set correct caption for form.
                $caption = lang::translate('blog_simple_delete_entry');
            } else {
                $caption = lang::translate('blog_simple_edit_entry');
            }
        } else {
            // else it is insert
            $tf->setMethod($method);
            // set correct caption
            $caption = lang::translate('blog_simple_add_entry');
        }

        // print the form
        $tf->createForm('', 'post',  'blog_simple form', '',
                           true,  $caption);
    }



    /**
     * we add a method for validating the submitted entry
     */
    public static function validate(){
        if (isset($_POST['submit'])){
            // very simple check. We just make sure something is
            // added to title and content fields.
            if (empty($_POST['title'])) self::$errors[] = lang::translate('blog_simple_no_title');
            if (empty($_POST['content'])) self::$errors[] = lang::translate('blog_simple_no_content');
        }
    }


    /**
     * we add a method for santizing the submitted entry
     * on every submission this is called
     */
    public static function sanitize(){
        if (isset($_POST['submit'])){
            // All ini settings is loaded from the file called
            // blog_simple.ini
            $filters = get_module_ini('blog_simple_filters');

            // we rewrite htmlentites
            $_POST['title'] = htmlentities($_POST['title']);

            // we run the content of our $_POST['content'] through all filters.
            $_POST['content'] = get_filtered_content($filters, $_POST['content']);
        }
    }

    /**
     * method for adding a blog entry
     * @return void
     */
    public static function addController(){
        // we set a title for the page where ths method is used
        template::setTitle(lang::translate('blog_simple_add_title'));

        // we check if user trying to add a blog entry is allowed to
        // if not we just return
        //
        // the session::checkAccessControl ('level') checks blog_simple.ini
        // to see what the setting blog_simple_edit equals. In our case
        // it equals 'admin'. Instead of setting the access level directly
        // we can now just edit our access restrictions in the blog_simple.ini

        if (!session::checkAccessControl('blog_simple_edit')){
            return;
        }

        // if any a form is submitted we check for errors and add the
        // the entry
        if (isset($_POST['submit'])){

            // validate and sanitize (see methods above)
            self::sanitize();
            self::validate();

            // if no errors
            if (empty(self::$errors)){

                // add entry
                $res = self::addEntry();

                // if success in adding entry
                if ($res){

                    // set action message (special message to inform the user
                    // when page is getting redirected.
                    session::setActionMessage(
                        // message on success is:
                        lang::translate('blog_simple_entry_created')
                    );
                    // redirect to index page
                    header("Location: /blog_simple/index");
                }
            // if errors we display the erros
            } else {
                // we use the view_form_errors, which is one of several
                // helper functions which can be added to a template file.
                view_form_errors(self::$errors);

                // display form after errors
                self::blogForm('insert');
            }
        } else {
            // no submisstion show blog entry form
            // the form will set correct values if form has
            // been submitted once
            self::blogForm('insert');
        }
    }


    /**
     * method for listing latest entries in our blog
     */
    public static function indexController (){

        // set title
        template::setTitle(lang::translate('blog_simple_view_all'));

        // we set this to 5
        // Note: It is set before pearPager.
        // otherwise it will be defined in pearPager.php with a alue of 10
        define('PAGER_PER_PAGE' , get_module_ini('blog_simple_per_page'));

        // we include pear pager, which is a wrapper class around the
        // PEAR_Pager class

        include_once "pearPager.php";

        // we use our db class to create a db object.
        $db = new db();

        // count all rows used for pagination
        $num_rows = $db->getNumRows('blog_simple');

        // all you need to tell pearPager object is the count of all numRows
        $pager = new pearPager($num_rows);

        // we select all and specify the ordering and the from and limit of the query.
        //
        // @param 'table'    (blog_simple)
        // @param fields     (array of fields) or null == ALL (*)
        // @param 'searh'    (e.g array ('user_id' => 1) or NULL (no search)
        // @param 'from'     (where in the row count to start our select)
        // @param 'limit'    (what is the limit
        // @param 'order_by' (which field do we order our select by) ==

        $rows = $db->selectAll('blog_simple', null, null, $pager->from, PAGER_PER_PAGE, 'updated');

        // simple template and the view file 'index.inc' parses all rows
        // and returns a string containing our list of blog entries.
        // not a real template system, but good enough when working with simple
        // rows
        
        $str = get_include_contents(_COS_PATH . "/modules/blog_simple/views/index.inc", $rows);

        // echo the string for display to screen
        echo $str;

        // print pagination data.
        $pager->pearPage();
    }

    /**
     * method for listing single full entry of our blog
     */
    public static function viewController (){

        // create a db object
        $db = new db();

        // select one (the same syntax as the above method, but this time
        // our object will only return one row.
        $row = $db->selectOne('blog_simple', null, array ('id' => self::getEntryId()));

        // we set a html title for the page where this method is used
        template::setTitle(lang::translate('blog_simple_view_entry') . " :: $row[title]");
        
        // we get filters set in blog_simple.ini
        $filters = get_module_ini('blog_simple_filters');

        // we filter content
        $row['content'] = get_filtered_content($filters, $row['content']);

        // and we use our simple template function for getting the string
        $str = get_include_contents(_COS_PATH . "/modules/blog_simple/views/view.inc", $row);
        
        // At last we echo the string 
        echo $str;
    }

    /**
     * note this is almost exactly the same function as the insertController
     * only difference is that we use an id for loading the form
     *
     * @return void
     */

    public static function updateController (){
        // we set a title for the page where ths method is used
        template::setTitle(lang::translate('blog_simple_update_entry'));

        // we check if user is trying to add a blog entry is allowed to
        // if not we just return
        if (!session::checkAccessControl('blog_simple_edit')){
            return;
        }

        // if any a form is submitted we check for errors and add the
        // the entry
        if (isset($_POST['submit'])){
            self::validate();
            self::sanitize();
            if (empty(self::$errors)){

                $res = self::updateEntry();
                if ($res){
                    session::setActionMessage(
                        lang::translate('blog_simple_entry_updated')
                    );
                    header("Location: /blog_simple/index");
                }
            } else {
                view_form_errors(self::$errors);
                self::blogForm('update', self::getEntryId());
            }
        } else {
            // no submisstion show blog entry form
            // the form will set correct values if form has
            // been submitted
            self::blogForm('update', self::getEntryId());
        }
    }


    /**
     * note this is almost exactly the same function as the insertController
     * only difference is that we use an id for loading the form
     *
     * @return void
     */

    public static function deleteController (){

        // we set a title for the page where ths method is used
        template::setTitle(lang::translate('blog_simple_delete_entry'));

        // we check if user is trying to add a blog entry is allowed to
        // if not we just return
        if (!session::checkAccessControl('blog_simple_allow')){
            return;
        }

        // if any a form is submitted we check for errors and add the
        // the entry
        if (isset($_POST['submit'])){

            $res = self::deleteEntry();
            if ($res){
                session::setActionMessage(
                    lang::translate('blog_simple_entry_deleted')
                );
                header("Location: /blog_simple/index");
            }
        } else {
            // no submisstion show blog entry form
            // the form will set correct values if form has
            // been submitted
            self::blogForm('delete', self::getEntryId());
        }

    }

    
    /**
     * method for inserting a blog entry into database
     *
     * @return  boolean true on succes false on failure
     */
    public static function addEntry (){

        $db = new db();

        // we add user id and updated fields to the _POST var
        $_POST['user_id'] = session::getUserId();
        $_POST['updated'] = date('Y-m-d H:i:s');

        // all values will be prepared
        // this just removes fields like 'submit' and 'captcha' from the
        // form submission
        $values = db::prepareToPost();

        // prepare and execute
        // insert the blog post
        $res = $db->insert('blog_simple', $values);

        // return boolean result from insert operation
        return $res;
    }

    /**
     * method for updating an entry
     * @return boolean
     */
    public static function updateEntry (){

        $db = new db();

        // we add user id and updated fields to the _POST var
        $_POST['user_id'] = session::getUserId();

        // all values will be prepared
        // this just removes fields like 'submit' and 'captcha' from the
        // form submission
        $values = db::prepareToPost();

        // prepare and execute
        // insert the blog post
        $res = $db->update('blog_simple', $values, self::getEntryId());

        // return boolean result from insert operation
        return $res;
    }

    /**
     * method for updating an entry
     * @return boolean
     */
    private static function deleteEntry (){
        $db = new db();
        $res = $db->delete('blog_simple', 'id', self::getEntryId());
        return $res;
    }

    /**
     * method for getting user info
     * @param   int     $id
     * @return  array   account row
     */
    public static function getUserInfo ($id){
        $db = new db();       
        $row = $db->selectOne('account', 'id', $id);
        return $row;
    }
}