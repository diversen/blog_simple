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

/**
 * This is just a trick. 
 * when making the next call we make it possible for all 'views' to be 
 * overriden in a template. 
 * 
 * All views is just functions calls, which makes it easy to navigate
 * in e.g. and IDE (like netbeans). You could also place views in a view folder 
 * and call them there - but this is what I prefer. 
 */
view::includeOverrideFunctions('blog_simple', 'views.phtml');

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
        // in this sense our url will look like this, e.g: blog_simple/index/123
        //
        // Note: The controllers are just files placed in the module dir
        // index.php refers to the 'index' part of the url.
        $id = uri::getInstance()->fragment(2);
        return filter_var($id, FILTER_VALIDATE_INT);
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
                echo formHelpers::confirmDeleteForm('submit',

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
            $_POST = html::specialEncode($_POST);

        }
    }

    
    /**
     * method for inserting a blog entry into database
     *
     * @return  boolean true on succes false on failure
     */
    public static function addEntry (){

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
    public static function updateEntry (){

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
    public static function deleteEntry (){
        $db = new db();
        $res = $db->delete('blog_simple', 'id', self::getEntryId());
        return $res;
    }
}
