<h3><?=$vars['title']?></h3>
<p><?php
$user = blogSimple::getUserInfo($vars['user_id']);

echo lang::translate('blog_simple_written_by') . " <strong>" . get_profile_link($user) . ".</strong> ";
$datetime = date_create($vars['updated']);
$date_formatted = date_format($datetime, register::$vars['coscms_main']['date_format_long']);

echo "<strong>$date_formatted</strong>\n";
?></p>

<p><?=$vars['content']?></p><?php

$ret = session::checkAccessControl('blog_simple_allow', false);
if ($ret) { ?>
<p>
<?=create_link("/blog_simple/edit/$vars[id]", lang::translate('blog_simple_edit_entry'))?> |
<?=create_link("/blog_simple/delete/$vars[id]", lang::translate('blog_simple_delete_entry'))?>
</p>
<?php

}
