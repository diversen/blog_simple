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
        // URI. The first fragment [0] is the module name
        // the next [1] is the controller name, and the third[2] is
        // an ID identifying the id of the blog post when we will read,
        // update or delete a post.
        //
        // in this sense our url will look like this, e.g:
	// blog_simple/index/123
        //
        // Note: The controllers are just files placed in the module dir
        // index.php refers to the 'index' part of the url.

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

        include_once "coslib/form_helpers.php";
        // we use html.php  from emthods, which is easy to use for creating forms connected
        // You don't need to use this class when you make your
        // modules. In fact you could use the Zend Form, Your own form class, or just write
        // the form as HTML and place it somewhere in the module dir.
        //
        // Though: For the uniform look and feel of a form, using these class makes it
        // quite easy to manipulate looks of the forms in the template.

        if (isset($id)){
            // if an id is set if update or delete
            if ($method == 'delete'){
                // simple delete form
                echo formHelpers::confirmDeleteForm(
                        lang::translate('blog_simple_delete_entry'));
                return;
            } else {
                // edit form
                // select one (the same syntax as the above method, but this time
                // params ('table' 'fields to fetch' simple search e.g. array('id' => 123),
                $db = new db();
                $values = $db->selectOne('blog_simple', 'id', self::getEntryId());
                $values = html::specialEncode($values);
                $legend = lang::translate('blog_simple_edit_entry');
            }
        } else {
            $legend = lang::translate('blog_simple_add_entry');
        }

        $form = new html();
        $form->formStart();
        $form->init($values, 'submit');
        $form->legend($legend);
        $form->label('title', lang::translate('blog_simple_title'));
        $form->text('title');
        $form->label('entry', lang::translate('blog_simple_title'));
        $form->textarea('entry');
        $form->submit('submit', lang::system('submit'));
        $form->formEnd();
        echo $form->getStr();
    }



    /**
     * we add a method for validating the submitted entry
     */
    public static function validate(){
        if (isset($_POST['submit'])){
            // very simple check. We just make sure something is
            // added to title and entry fields.
            if (empty($_POST['title'])) {
                self::$errors[] = lang::translate('blog_simple_no_title');
            }
            if (empty($_POST['entry'])) {
                self::$errors[] = lang::translate('blog_simple_no_entry');
            }
        }
    }


    /**
     * we add a method for santizing the submitted entry
     * on every submission this is called
     */
    public static function sanitize(){
        if (isset($_POST['submit'])){

            // we rewrite htmlentites
            // cos_htmlentites is the same htmlentites, but this
            // function works on arrays too.
            $_POST = html::specialEncode($_POST);

        }
    }

    /**
     * method for adding a blog entry. This is the add controller.
     * we will make a controller file where this function is called.
     *
     * the controller file is placed in top of our module dir, and
     * it will be called add.php which refers to the uri:
     *
     * blog_simple/add
     *
     * the controller file will only contain the following static
     * method call:
     *
     * blogSimple::addController()
     *
     * @return void
     */
    public static function addController(){

        // we set a title for the page where this method is used
        // this is the <title> tag. in the page.
        template::setTitle(lang::translate('blog_simple_add_title'));

        // we check if user trying to add a blog entry is allowed to
        // if not we just return
        //
        // the session::checkAccessControl ('level') checks blog_simple.ini
        // to see what the setting blog_simple_edit equals. In our case
        // it equals 'admin'. Instead of setting the access level directly
        // we can now just edit our access restrictions in the blog_simple.ini
        // file. The session::checkAccessControl will also flag a 403 if
        // the user in session does not meet the required access level.
        // and will be redirected to a 403 error file.

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

                // add entry, but decode htmlentities. 
                $_POST = html::specialDecode($_POST);
                $res = self::addEntry();

                // if success in adding entry - redirect
                if ($res){
                    http::locationHeader(
                            "/blog_simple/index",
                            lang::translate('blog_simple_entry_created')
                            );
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
     *
     * As in the above controller method:
     *
     * the controller file is placed in top of our module dir, and
     * it will be called index.php which refers to the uri:
     *
     * blog_simple/index
     *
     * the index file will only contain the following static
     * method call:
     *
     * blogSimple::indexController()
     *
     */
    public static function indexController (){

        // set title
        template::setTitle(lang::translate('blog_simple_view_all'));

        // We define how many items we want to show per page.
        // This is defined in our ini file.
        //
        // Note: It is set before pearPager.
        // otherwise it will be defined in pearPager.php with a value of 10
        $per_page = config::getModuleIni('blog_simple_per_page');

        // we include pear pager, which is a wrapper class around the
        // PEAR_Pager class. We use include_once because maybe some
        // other module has already included the file.

        include_once "coslib/pearPager.php";

        // get a count of all rows in blog_simple_table
        $num_rows = dbQ::setSelectNumRows('blog_simple')->
                fetch();

        // all you need to tell pearPager object is the count of all numRows
        $pager = new pearPager($num_rows, $per_page);

        // select with queryBuilder - a simple ORM
        $rows = dbQ::setSelect('blog_simple')->
                order('updated')->
                limit($pager->from, PAGER_PER_PAGE)->
                fetch();

        // simple template view file 'index.inc' parses all rows
        // and returns a string containing our list of blog entries.
        // it just uses php as a template engine.
        // we will look at this template in the next part of the tutorial
        // all you need to know for now is that we just pass all selected
        // rows to the template placed in blog_simple/modules/views
        
        echo view::get('blog_simple', 'index', $rows);

        // and we print the pagination data.
        $pager->pearPage();
    }

    /**
     * method for listing single full entry of our blog
     *
     * the method call will be used in the file
     * blog_simple/view.php
     *
     * it displays a full blog entry in this style , e.g.:
     *
     * blog_simple/view/123
     *
     * which will get us blog entry number 123 and display it
     * for the reader
     *
     */
    public static function viewController (){

        // create a db object
        $db = new db();

        // select one (the same syntax as the above method, but this time
        // params ('table' 'fields to fetch' simple search e.g. array('id' => 123),
        $row = $db->selectOne('blog_simple', 'id', self::getEntryId());

        // we set a html title for the page where this method is used
        template::setTitle(lang::translate('blog_simple_view_entry') . " :: $row[title]");
        
        // we get filters set in blog_simple.ini
        // markdown (located in modules/filter_markdown/markdown.inc)
        $filters = get_module_ini('blog_simple_filters');

        // we filter content with a helper function called get_filtered_entry
        // which filters entry with all filters specified in the array $filters
        $row['entry'] = get_filtered_content($filters, $row['entry']);

        // and we use our simple template function for getting the string
        // this time our template is located in views/view.inc
        $str = view::get('blog_simple', 'view', $row);
        
        // At at last we echo the string
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

                // decode htmlentities again.
                $_POST = html::specialDecode($_POST);
                $res = self::updateEntry();
                if ($res){
                    // redirect and set action message
                    // which will be displayed on next page.
                    http::locationHeader(
                            '/blog_simple/index', 
                            lang::translate('blog_simple_entry_updated'));
                }
            } else {
                view_form_errors(self::$errors);
                
            }
        }
        self::blogForm('update', self::getEntryId());
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
                http::locationHeader(
                        "/blog_simple/index",
                        lang::translate('blog_simple_entry_deleted')
                    );
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
        // form submission. Also note that we have performed
        // htmlentites on all fields

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
    public static function deleteEntry (){
        $db = new db();
        $res = $db->delete('blog_simple', 'id', self::getEntryId());
        return $res;
    }
}
