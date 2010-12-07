<?php

/**
 * This is our model file. In this file we place all logic.
 * You could place html excentric files in view.blog_simple.inc
 * Or you could make a directory like modules/blog_simple/views
 *
 * This is up to you.
 *
 * In order to make this tutorial simple we place almost anything in
 * the model file.
 */

// we include mysqlForm which is easy to use for creating forms connected
// to a database
include_once "mySqlForm.php";

class blogSimple {

    /**
     *
     * @var array   array for holding error codes
     */
    public static $errors = array();


    /**
     * method for getting a filtered entry id.
     * @return int  returns filtered int
     */
    public static function getEntryId (){
        return filter_var(uri::$fragments[2], FILTER_VALIDATE_INT);
    }

    /**
     * function for creating a form for insert, update and deleting
     * blog entries. why is it static. I like static, it is a faster
     * the public methods. So if I can use static methods, I usually do.
     *
     * @param string    method (update, delete or insert)
     * @param int       id (if delete or update)
     * @param array     array with values if we load the form for
     *                  an update operation
     */
    public static function blogForm($method, $id = null, $values = array()){
        // this is the forms fields captions
        // as you can see we make our module languag aware,
        // so we can easily translate the module strings
        $fields_captions =
            array(
                'title' => lang::translate('blog_simple_title'),
                'content' => lang::translate('blog_simple_content'),
        );


        // which fields do we use
        $fields = array('id', 'title', 'content');

        // init the for with fields and specify which
        // table to use for our form
        $tf = new mySqlForm('blog_simple', $fields, $values);

        // set the labels
        $tf->setLabels($fields_captions);

        // if an id is set it must be update or delete
        if (isset($id)){

            $tf->setMethod($method, $id);
            if ($method == 'delete'){
                $caption = lang::translate('blog_simple_delete_entry');
            } else {
                $caption = lang::translate('blog_simple_edit_entry');
            }
        } else {
            $tf->setMethod($method);
            $caption = lang::translate('blog_simple_add_entry');
        }

        // print the form
        $tf->createForm('', 'post',  'article form', '',
                           true,  $caption);
    }



    /**
     * we add a method for validating the submitted entry
     */
    public static function validate(){
        if (isset($_POST['submit'])){
            if (empty($_POST['title'])) self::$errors[] = lang::translate('blog_simple_no_title');
            if (empty($_POST['content'])) self::$errors[] = lang::translate('blog_simple_no_content');
        }
    }


    /**
     * we add a method for santizing the submitted entry
     */
    public static function sanitize(){
        if (isset($_POST['submit'])){
            $filters = get_module_ini('blog_simple_filters');
            $_POST['title'] = cos_htmlentities($_POST['title']);
            $_POST['content'] = get_filtered_content($filters, $_POST['content']);
            if (empty($_POST['title'])) self::$errors[] = lang::translate('blog_simple_no_title');
            if (empty($_POST['content'])) self::$errors[] = lang::translate('blog_simple_no_content');
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
        if (!session::checkAccessControl('blog_simple_edit')){
            return;
        }

        // if any a form is submitted we check for errors and add the
        // the entry
        if (isset($_POST['submit'])){
            self::sanitize();
            self::validate();
            if (empty(self::$errors)){
                
                $res = self::addEntry();
                if ($res){
                    session::setActionMessage(
                        lang::translate('blog_simple_entry_created')
                    );
                    header("Location: /blog_simple/index");
                }
            } else {
                view_form_errors(self::$errors);
                self::blogForm('insert');
            }
        } else {
            // no submisstion show blog entry form
            // the form will set correct values if form has
            // been submitted
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
        include_once "pearPager.php";

        $db = new db();

        // count all rows used for pagination
        $num_rows = $db->getNumRows('blog_simple');

        // pearPager class is a wrapper around the pear Pager.
        $pager = new pearPager($num_rows);

        // select all             'table' 'fields' 'searh'   'from'      'limit'       'order_by'
        $rows = $db->selectAll('blog_simple', null, null, $pager->from, PAGER_PER_PAGE, 'updated');

        // simple template and the view file 'index.inc' parses all rows
        // and returns a string containing our list of blog entries.
        // not a real template system, but good enough when working with simple
        // rows
        
        $str = get_include_contents(_COS_PATH . "/modules/blog_simple/views/index.inc", $rows);
        print $str;

        // print pagination data.
        $pager->pearPage();
    }

    /**
     * method for listing latest entries in our blog
     */
    public static function viewController (){


        $db = new db();

        // select
        $row = $db->selectOne('blog_simple', null, array ('id' => self::getEntryId()));

        // we set a title for the page where ths method is used
        template::setTitle(lang::translate('blog_simple_view_entry') . " :: $row[title]");
        
        // get filters set in blog_simple.ini
        $filters = get_module_ini(('blog_simple_filters'));

        // we filter content
        $row['content'] = get_filtered_content($filters, $row['content']);

        $str = get_include_contents(_COS_PATH . "/modules/blog_simple/views/view.inc", $row);
        print $str;
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
