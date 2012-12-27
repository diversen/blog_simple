<?php

/**
 * view file for listing all entries by updated date.
 *
 * In the mdoel we have the method blogSimple::indexController()
 * which will list all entries in our blog.
 *
 * We used the template function get_include_contents()
 * in this manner:
 *
 * $str = get_include_contents(_COS_PATH . "/modules/blog_simple/views/index.inc", $rows);
 *
 * In this template all rows are transformed into the variable $vars
 *
 * We also use a couple of standard functions, which all can be found in
 * lib/common.inc
 */

foreach ($vars as $key => $row) {  
    
    $link_url = "/blog_simple/view/$row[id]/";
    $link_url.= strings::utf8SlugString($row['title']);
    $link = html::createLink($link_url, $row['title']);

    // we print headline for our blog entry
    headline_message ($link);

    $date = time::getDateString($row['updated']);
    echo user::getProfileSimple($row['user_id'], $date);

    // we fetch a module setting telling us how long our abstract
    // should be
    $teaser_length = get_module_ini('blog_simple_abstract_length');

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
