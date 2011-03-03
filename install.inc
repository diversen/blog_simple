<?php

// This sets the version of the module.
// Note that the version follows sql.
//
// If we wanted to update our SQL at a later point
// we will add a fill to the 'up' dir where we place a
// e.g. 1.02.sql file and update $_INSTALL['VERSION] as well.
$_INSTALL['VERSION'] = 1.02;

// This creates a main menu item in the database
$_INSTALL['MAIN_MENU_ITEM'] = array (
    'title' => 'blog_simple_title',
    'url' => '/blog_simple/index',
    'module_name' => 'blog_simple',
    'parent' => 0,
    'weight' => 2,
);

// This sets the public clone URL
$_INSTALL['PUBLIC_CLONE_URL'] = "git://github.com/diversen/blog_simple.git";
$_INSTALL['PRIVATE_CLONE_URL'] = 'git@github.com:diversen/blog_simple.git';
