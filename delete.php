<?php

class blogSimpleDelete extends blogSimple {
    
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
}

blogSimpleDelete::deleteController();
