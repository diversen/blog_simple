<?php

// print title
headline_message($vars['title']);

// start paragraph
echo "<p>\n";

// get user info for this blog entry
//$user = blogSimple::getUserInfo($vars['user_id']);

// compose written by string
// $written_by = lang::translate('blog_simple_written_by');
// $written_by.= " <strong>" . get_profile_link($user) . "</strong> ";

// echo written by
//echo $written_by;

// compose formatted date as string
// $datetime = strtotime($vars['updated']);
$date_formatted = time::getDateString($vars['updated']);

// echo formatted date
//echo "<strong>$date_formatted</strong>";

//$unix_stamp = strtotime($vars['updated']);
//$date = strftime(get_main_ini('date_format_long'), $unix_stamp);
  
//user::getProfileSimple($user, $date);
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
