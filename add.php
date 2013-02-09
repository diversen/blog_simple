<?php

class blogSimpleAdd extends blogSimple {
    /**
     * method for adding a blog entry. This is the add controller.
     * we will make a controller file where this function is called.
     *
     * the controller file is placed in top of our module dir, and
     * it will be called add.php which refers to this URL:
     *
     * /blog_simple/add
     *
     * the controller file will only contain the following static
     * method call:
     *
     * blogSimpleAdd::addController()
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
                html::errors(self::$errors);

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
}

blogSimpleAdd::addController();
