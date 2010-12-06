<?php

$_MODULE_MENU[] = array(
    'title' => lang::translate('blog_simple_menu_list'),
    'url' => '/blog_simple/index',
);
$blog_simple_allow = get_module_ini('blog_simple_allow');
$_MODULE_MENU[] = array(
    'title' => lang::translate('blog_simple_menu_add'),
    'url' => '/blog_simple/add',
    'auth' => $blog_simple_allow
);