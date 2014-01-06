<?php

/**
 * This is where we define our menus.
 */

// First menu item is where we plan to list all entries.
// As you can see the module menu just specifies links
// to controller files. index refers to index.php in our
// module folder
$_MODULE_MENU[] = array(
    'title' => lang::translate('View all'),
    'url' => '/blog_simple/index',
);

// we read who is allowed to use admin interface
// which we get from an ini setting. We will see this ini
// file in the next section of the tutorial.

$blog_simple_allow = config::getModuleIni('blog_simple_allow');

// we add a menu entry for adding entries. Again it is just
// a link with URL and title, but notice that we set an auth
// element of the array in addition.
// This just means that it is only users logged in as admin
// users who will see this link. Anybody else does not need to
// see this link.
$_MODULE_MENU[] = array(
    'title' => lang::translate('Add'),
    'url' => '/blog_simple/add',
    // note that we have set an auth element!
    'auth' => $blog_simple_allow
);
