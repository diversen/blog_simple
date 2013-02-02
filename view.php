<?php

class blogSimplView extends blogSimple {
        /**
     * method for listing single full entry of our blog
     *
     * the method call will be used in the file
     * blog_simple/view.php
     *
     * it displays a full blog entry in this style , e.g.:
     *
     * blog_simple/view/123
     *
     * which will get us blog entry number 123 and display it
     * for the reader
     *
     */
    public static function viewController (){

        // create a db object
        $db = new db();

        // select one (the same syntax as the above method, but this time
        // params ('table' 'fields to fetch' simple search e.g. array('id' => 123),
        $row = $db->selectOne('blog_simple', 'id', self::getEntryId());

        // we set a html title for the page where this method is used
        template::setTitle(lang::translate('blog_simple_view_entry') . " :: $row[title]");
        
        // we get filters set in blog_simple.ini
        // markdown (located in modules/filter_markdown/markdown.inc)
        $filters = get_module_ini('blog_simple_filters');

        // we filter content with a helper function called get_filtered_entry
        // which filters entry with all filters specified in the array $filters
        $row['entry'] = moduleloader::getFilteredContent($filters, $row['entry']);

        // call the 'view' function
        blog_simple_view_entry($row);

    }
}

blogSimplView::viewController();
