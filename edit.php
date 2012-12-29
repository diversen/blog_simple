<?php

class blogSimpleUpdate extends blogSimple {

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
}

blogSimpleUpdate::updateController();
