<?php

// view file for listing all entries by date.
// 
// As you may remember from the model file in the method
// blogSimple::indexController()
//
// This was the method call used to get a string with latest entries.
//
// $str = get_include_contents(_COS_PATH . "/modules/blog_simple/views/index.inc", $rows);
//
// In this template all rows are transformed into the variable $vars
//
// a couple of new functions are used:
//
// create_link ($url, $title) // function that generates a html link
// create_seo_title ($title) function that creates SEO friendly title from a string
// substr2 ($text, $length) extracts a portion of the text specified by $length
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