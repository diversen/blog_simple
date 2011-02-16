<?php

// print title
echo "<h3>$vars[title]</h3>";

// start paragraph
echo "<p>\n";

// get user info for this blog entry
$user = blogSimple::getUserInfo($vars['user_id']);

// compose written by string
$written_by = lang::translate('blog_simple_written_by');
$written_by.= " <strong>" . get_profile_link($user) . "</strong> ";

// echo written by
echo $written_by;

// compose formatted date as string
$datetime = date_create($vars['updated']);
$date_formatted = date_format($datetime, register::$vars['coscms_main']['date_format_long']);

// echo formatted date
echo "<strong>$date_formatted</strong>";

// echo content
echo $vars['content'];

// check if it is an amdin user and add edit options
$ret = session::checkAccessControl('blog_simple_allow', false);
if ($ret) {
    echo create_link("/blog_simple/edit/$vars[id]", lang::translate('blog_simple_edit_entry'));
    echo " | ";
    echo create_link("/blog_simple/delete/$vars[id]", lang::translate('blog_simple_delete_entry'));
}