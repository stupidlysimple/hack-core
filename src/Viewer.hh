<?hh
/**
 * StupidlySimple Framework for Hack - A Framework For Hack-ers
 *
 * Copyright (c) 2017 Fariz Luqman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @package     StupidlySimple Framework for Hack
 * @author      Fariz Luqman <fariz.fnb@gmail.com>
 * @copyright   2017 Fariz Luqman
 * @license     MIT
 * @since       0.1.0
 * @link        https://stupidlysimple.github.io/hack/
 */
class Viewer {
    /**
     * Finds, renders and displays a template file. Reports a 404 error in
     * case of missing files.
     *
     * @param string	$file		file name / path to the file
     * @param array		&$data	array of references to data or objects
     *
     * @static
     * @access public
     * @see Viewer::render()
     * @since Method available since Release 0.1.0
     */
    static function file($file, array &$data = []) : void{
        // Do you love displaying blank pages?
        if($file === 'index' || $file === 'index.hh'){
            Debugger::report(404, true);
        }else{
            /**
             * Get the path of the calling script and get it's containing Directory
             * to enable include() style of accessing files
             */
            if(file_exists(SS_PATH.$file)){
                self::render(SS_PATH.$file, $data);
            }else if(file_exists(SS_PATH.'resources/views/'.$file)){
                self::render(SS_PATH.'resources/views/'.$file, $data);
            }else if(file_exists(SS_PATH.'resources/views/'.$file.'.hh')){
                self::render(SS_PATH.'resources/views/'.$file.'.hh', $data);
            }else if(file_exists(SS_PATH.$file.'.hh')){
                self::render(SS_PATH.$file.'.hh', $data);
            }else if(file_exists(SS_PATH.$file)){
                self::render($file, $data);
            }else if(file_exists(SS_PATH.$file.'.hh')){
                self::render(SS_PATH.$file.'.hh', $data);
            }else{
                Debugger::report(404, true);
            }
        }
    }
    /**
     * Renders a template file. Inject dependencies from the Application
     * Container and the Core\Sharer before viewing the file. Also,
     * extracts &$data into variables usable from the template files
     *
     * @param string	$file		file name / path to the file
     * @param array		&$data	array of data or objects
     *
     * @static
     * @access private
     * @since Method available since Release 0.1.0
     */
    static private function render($file, &$data = []) : void {
        // Extract the variable $data passed to the Core\Viewer::file()
        extract ($data);
        // We don't want duplicated data
        unset($data);
        // Extract data retreived from the Sharer
        if(Sharer::get() !== null){
            extract(Sharer::get());
        }
        ob_start();
        include_once($file);
        $output = ob_get_contents();
        ob_end_clean();
        echo ($output);
    }
}