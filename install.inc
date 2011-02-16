<?php

// This sets the version of the module
$_INSTALL['VERSION'] = 1.01;

// This creates a main menu item in the database
$_INSTALL['MAIN_MENU_ITEM'] = array (
    'title' => 'blog_simple_title',
    'url' => '/blog_simple/index',
    'module_name' => 'blog_simple',
    'parent' => 0,
    'weight' => 2,
);

// This sets a public clone ulr
$_INSTALL['PUBLIC_CLONE_URL'] = "git://github.com/diversen/blog_simple.git";