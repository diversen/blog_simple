<?php

/**
 * This is our module file. 
 */

/**
 * First thing is a nice trick
 * . 
 * when making the next call we make it possible for all 'views' to be 
 * overriden in a template. 
 * 
 * All views in this example are just functions calls, which makes it easy to navigate
 * in e.g. and IDE (like netbeans). You could also place views in a view folder 
 * and call them there - but this is what I prefer. 
 */
view::includeOverrideFunctions('blog_simple', 'views.php');

class blog_simple {

    /**
     * A simple static variable holding error codes and messages.
     * @var array   array for holding error codes
     */
    public static $errors = array();


    /**
     * method for getting a filtered entry id.
     * @return int  returns filtered int
     */
    public function getEntryId (){

        // We will use the uri class for getting info about the
        // URI. The first fragment [0] is the module name
        // the next [1] is the action name, and the third[2] is
        // an ID identifying the id of the blog post when we will read,
        // update or delete a post.
        //
        // in this sense our url will look like this, e.g: blog_simple/index/123
        //
        $id = uri::fragment(2);
        
        // we validate it as int before returning it. 
        return filter_var($id, FILTER_VALIDATE_INT);
    }

    /**
     * function for creating a form for insert, update and deleting
     * blog entries. This could have been place in the blog_simple_views
     * class as we then would be able to override this in a template.
     *
     * @param string    $method (update, delete or insert)
     * @param int       $id (if delete or update)
     * @param array     $values array with values if we load the form for
     *                  an update operation
     */
    public function form($method, $id = null, $values = array()){

        // we use html.php  from emthods, which is easy to use for creating 
        // forms connected to a database
        // 
        // You don't need to use this class when you make your
        // modules. In fact you could use the Zend Form, Your own form class, or just write
        // the form as HTML and place it somewhere in the module dir.
        //
        // Though: For the uniform look and feel of a form, using these class 
        // makes it quite easy to manipulate looks of the forms in the template.

        if (isset($id)){
            // if an id is set if update or delete
            if ($method == 'delete'){
                //$helper = new formHelpers();
                echo html_helpers::confirmDeleteForm('submit',

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
    public function validate(){
        if (isset($_POST['submit'])){
            // very simple check. We just make sure something is
            // added to title and entry fields.
            if (empty($_POST['title'])) {
                $this->errors[] = lang::translate('blog_simple_no_title');
            }
            if (empty($_POST['entry'])) {
                $this->errors[] = lang::translate('blog_simple_no_entry');
            }
        }
    }


    /**
     * we add a method for santizing the submitted entry
     * on every submission this is called
     */
    public function sanitize(){
        if (isset($_POST['submit'])){

            // we rewrite htmlentites
            $_POST = html::specialEncode($_POST);

        }
    }

    
    /**
     * method for inserting a blog entry into database
     *
     * @return  boolean true on succes false on failure
     */
    public function addEntry (){

        // we add user id and updated fields to the _POST var
        $_POST['user_id'] = session::getUserId();

        // all values will be prepared
        // this just removes fields like 'submit' and 'captcha' from the
        // form submission. Also note that we have performed
        // htmlentites on all fields

        $values = db::prepareToPost();

        // prepare and execute
        // insert the blog post
        
        $db = new db();
        $res = $db->insert('blog_simple', $values);

        // return boolean result from insert operation
        return $res;
    }

    /**
     * method for updating an entry
     * @return boolean
     */
    public function updateEntry (){

        // we add user id and updated fields to the _POST var
        $_POST['user_id'] = session::getUserId();

        // all values will be prepared
        // this just removes fields like 'submit' and 'captcha' from the
        // form submission
        $values = db::prepareToPost();

        // prepare and execute
        // insert the blog post
        
        $db = new db();
        $res = $db->update('blog_simple', $values, self::getEntryId());

        // return boolean result from insert operation
        return $res;
    }

    /**
     * method for updating an entry
     * @return boolean
     */
    public function deleteEntry (){
        $db = new db();
        $res = $db->delete('blog_simple', 'id', self::getEntryId());
        return $res;
    }
    
            /**
     * method for listing latest entries in our blog
     *
     * the controller file is placed in top of our module dir, and
     * it will be called index.php which refers to this URL:
     *
     * blog_simple/index
     *
     * the index file will only contain the following static
     * method call:
     *
     * blogSimpleIndex::indexController()
     *
     */
    public function indexAction (){

        // set title
        template::setTitle(lang::translate('blog_simple_view_all'));

        // We define how many items we want to show per page.
        // This is defined in our ini file.
        //
        // Note: It is set before pearPager.
        // otherwise it will be defined in pearPager.php with a value of 10
        $per_page = config::getModuleIni('blog_simple_per_page');

        // get a count of all rows in blog_simple_table
        $num_rows = db_q::numRows('blog_simple')->
                fetch();

        // all you need to tell pearPager object is the count of all numRows
        $pager = new paginate($num_rows, $per_page);

        // select with queryBuilder - a simple ORM
        $rows = db_q::select('blog_simple')->
                order('updated')->
                limit($pager->from, $per_page)->
                fetch();

        // encode rows
        $rows = html::specialEncode($rows);
        
        // simple template view file 'index.inc' parses all rows
        // and returns a string containing our list of blog entries.
        // it just uses php as a template engine.
        // we will look at this template in the next part of the tutorial
        // all you need to know for now is that we just pass all selected
        // rows to the template placed in blog_simple/modules/views
        
        blog_simple_views::index($rows);
        
        // same thing with a view file: see views/index
        //echo view::get('blog_simple', 'index', $rows);

        // and we print the pagination data.
        echo $pager->getPagerHTML();
        
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
    public function viewAction (){

        // create a db object
        $db = new db();

        // select one (the same syntax as the above method, but this time
        // params ('table' 'fields to fetch' simple search e.g. array('id' => 123),
        $row = $db->selectOne('blog_simple', 'id', self::getEntryId());

        // we set a html title for the page where this method is used
        template::setTitle(lang::translate('blog_simple_view_entry') . " :: $row[title]");
        
        // call the 'view' function
        blog_simple_views::view($row);

    }
    
    /**
     * method for adding a blog entry. This is the add controller.
     * This is is used when user hits /blog_simple/add
     *
     * @return void
     */
    public function addAction(){

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
            $this->sanitize();
            $this->validate();

            // if no errors
            if (empty($this->errors)){

                // add entry, but decode htmlentities. 
                $_POST = html::specialDecode($_POST);
                $res = $this->addEntry();

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
                html::errors($this->errors);

                // display form after errors
                $this->form('insert');
            }
        } else {
            // no submisstion show blog entry form
            // the form will set correct values if form has
            // been submitted once
            $this->form('insert');
        }
    }
    
    /**
     * note this is almost exactly the same function as the insertController
     * only difference is that we use an id for loading the form
     *
     * @return void
     */

    public function editAction (){
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
            $this->validate();
            $this->sanitize();
            if (empty($this->errors)){

                // decode htmlentities again.
                $_POST = html::specialDecode($_POST);
                $res = $this->updateEntry();
                if ($res){
                    // redirect and set action message
                    // which will be displayed on next page.
                    http::locationHeader(
                            '/blog_simple/index', 
                            lang::translate('blog_simple_entry_updated'));
                }
            } else {
                html::errors($this->errors);
                
            }
        }
        $this->form('update', self::getEntryId());
    }
    
    /**
     * note this is almost exactly the same function as the insertController
     * only difference is that we use an id for loading the form
     *
     * @return void
     */
    public function deleteAction (){

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
            $res = $this->deleteEntry();
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
            $this->form('delete', self::getEntryId());
        }
    }
}
