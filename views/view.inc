<?php

// echo title
html::headline($vars['title']);

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
