<?php

// view file for listing all entries by date.
// the substr2 takes an abstract from the content row
// and uses 150 chars to display a abstract

foreach ($vars as $key => $row) { ?>
<h3><?=create_link("/blog_simple/view/$row[id]/" . create_seo_title($row['title']), $row['title'])?></h3>
<p><?php

$user = blogSimple::getUserInfo($row['user_id']);

echo lang::translate('blog_simple_written_by') . " <strong>" . get_profile_link($user) . ".</strong> ";
$datetime = date_create($row['updated']);
$date_formatted = date_format($datetime, register::$vars['coscms_main']['date_format_long']);

echo "<strong>$date_formatted</strong>\n";

?></p>
<p><?=substr2($row['content'], get_module_ini('blog_simple_abstract_length'), 3)?></p><?php
$ret = session::checkAccessControl('blog_simple_allow', false);

if ($ret){ ?>
<p>
<?=create_link("/blog_simple/edit/$row[id]", lang::translate('blog_simple_edit_entry'))?> |
<?=create_link("/blog_simple/delete/$row[id]", lang::translate('blog_simple_delete_entry'))?>
</p>
<?php

    }
}