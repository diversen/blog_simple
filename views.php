<?php

/**
 * view methods are placed in blog_simple_views class
 */
class blog_simple_views {
    
    /**
     * view for index action
     * @param array $vars
     */
    public static function index ($vars = array ()) {
        foreach ($vars as $row) {  

            $link_url = "/blog_simple/view/$row[id]/";
            $link_url.= strings::utf8SlugString($row['title']);
            $link = html::createLink($link_url, $row['title']);

            // we print headline for our blog entry
            html::headline ($link);

            $date = time::getDateString($row['updated']);
            echo user::getProfileSimple($row['user_id'], $date);

            // we fetch a module setting telling us how long our abstract
            // should be
            $teaser_length = config::getModuleIni('blog_simple_abstract_length');

            // we use the standard function substr2 to get our teaser of the entry
            $teaser = strings::substr2($row['entry'], $teaser_length, 3);

            // print teaser to screen
            echo "<p>$teaser</p>\n";

            // we check if user has access right to view admin options of this
            // entry. (as a standard all access control function can be found
            // in the session class located in lib/session.php
            $ret = session::checkAccessControl('blog_simple_allow', false);

            // if access is ok we print some admin links.
            if ($ret){
                echo html::createLink("/blog_simple/edit/$row[id]", lang::translate('blog_simple_edit_entry'));
                echo MENU_SUB_SEPARATOR;
                echo html::createLink("/blog_simple/delete/$row[id]", lang::translate('blog_simple_delete_entry'));
            }
        }     
    }
    
    /**
     * view for view action
     * @param array $vars
     */
    public static function view ($vars) {

        // echo title
        // we get filters set in blog_simple.ini
        // markdown (located in modules/filter_markdown/markdown.inc)
        $filters = config::getModuleIni('blog_simple_filters');

        // we filter content with a helper function called get_filtered_entry
        // which filters entry with all filters specified in the array $filters
        $vars['entry'] = moduleloader::getFilteredContent($filters, $vars['entry']);
        
        
        html::headline(html::specialEncode($vars['title']));

        // start paragraph
        echo "<p>\n";

        // compose formatted date as string
        // $datetime = strtotime($vars['updated']);
        $date_formatted = time::getDateString($vars['updated']);

        // get simple profile
        echo user::getProfileSimple($vars['user_id'], $date_formatted);

        // echo entry
        echo $vars['entry'];

        // check if it is an amdin user and add edit options
        $ret = session::checkAccessControl('blog_simple_allow', false);
        if ($ret) {
            echo html::createLink("/blog_simple/edit/$vars[id]", lang::translate('blog_simple_edit_entry'));
            echo MENU_SUB_SEPARATOR;
            echo html::createLink("/blog_simple/delete/$vars[id]", lang::translate('blog_simple_delete_entry'));
        }
    }
}