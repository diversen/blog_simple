<?php

class blogSimpleIndex extends blogSimple {
    /**
     * method for listing latest entries in our blog
     *
     * the controller file is placed in top of our module dir, and
     * it will be called index.php which refers to this URL:
     *
     * blog_simple/index
     *
     * the index file will only contain the following static
     * method call:
     *
     * blogSimpleIndex::indexController()
     *
     */
    public static function indexController (){

        // set title
        template::setTitle(lang::translate('blog_simple_view_all'));

        // We define how many items we want to show per page.
        // This is defined in our ini file.
        //
        // Note: It is set before pearPager.
        // otherwise it will be defined in pearPager.php with a value of 10
        $per_page = config::getModuleIni('blog_simple_per_page');

        // we include pear pager, which is a wrapper class around the
        // PEAR_Pager class. We use include_once because maybe some
        // other module has already included the file.

        include_once "coslib/pearPager.php";

        // get a count of all rows in blog_simple_table
        $num_rows = dbQ::setSelectNumRows('blog_simple')->
                fetch();

        // all you need to tell pearPager object is the count of all numRows
        $pager = new pearPager($num_rows, $per_page);

        // select with queryBuilder - a simple ORM
        $rows = dbQ::setSelect('blog_simple')->
                order('updated')->
                limit($pager->from, $per_page)->
                fetch();

        // simple template view file 'index.inc' parses all rows
        // and returns a string containing our list of blog entries.
        // it just uses php as a template engine.
        // we will look at this template in the next part of the tutorial
        // all you need to know for now is that we just pass all selected
        // rows to the template placed in blog_simple/modules/views
        
        blog_simple_view_index($rows);
        
        // same thing with a view file: see views/index
        //echo view::get('blog_simple', 'index', $rows);

        // and we print the pagination data.
        $pager->pearPage();
        
    }       
}

blogSimpleIndex::indexController();
