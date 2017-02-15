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
namespace Core;

/**
 *  The Debugger
 * -----------------------------------------------------------------------
 *
 * Provides the developer with useful messages in case of an exception or
 * errors happen.
 *
 */
class Debugger {
    private static $profiles = [];
    private static $time_start = 0;
    private static $profilerStartTime = 0;

    /**
     * Registering the debugger to log exceptions locally or transfer them to
     * external services.
     *
     * Depends on the settings in config/env.hh:
     *
     * + 0: Shows "Something went wrong" message ambiguously (handled locally)
     *
     * + 1:	Shows simple error message, file and the line occured (handled
     *			locally)
     *
     * + 2: Shows advanced debugging with code snippet, stack frames, and
     *			envionment details, handled by Flip\Whoops
     *
     * @static
     * @access public
     * @since Method available since Release 0.1.0
     */
    static function start() : void {
        register_shutdown_function('Core\Debugger::error_handler');
        set_error_handler('Core\Debugger::error_handler');
        set_exception_handler('Core\Debugger::error_handler');
    }

    /**
     * Sets the header of the HTTP request and then display the
     * HTTP error codes.
     *
     * @param string	$code				The HTTP error code
     * @param bool		$terminate	Terminate the entire script execution
     *
     * @static
     * @see Debugger::set_header(), Debugger::display()
     * @access public
     * @since Method available since Release 0.1.0
     */
    static function report($code, $terminate = false) : void {
        switch ($code) {
            case '404':
                self::set_header('404', 'Internal Server Error');
                self::display('simple', '404 Not Found');
                break;
            case '500':
                self::set_header('500', 'Internal Server Error');
                self::display('simple', 'Something went wrong');
                break;
            default:
                self::set_header('500', 'Internal Server Error');
                self::display('simple');
                break;
        }

        if($terminate){
            die();
        }
    }

    /**
     * Sets the header of the HTTP request
     *
     * @static
     * @access public
     * @since Method available since Release 0.1.0
     */
    static function set_header($code, $error) : void {
        header($_SERVER['SERVER_PROTOCOL']. ''. $code . '' . $error);
    }

    /**
     * The error handler which is called by register_shutdown_function()
     * in event of exceptions, syntax errors, warning and notices.
     *
     * @static
     * @see Debugger::start(), Debugger::display()
     * @access public
     * @since Method available since Release 0.1.0
     */
    static function error_handler() : void {
        $error = error_get_last();
        $message = $error['message'];
        if($error){
            if(getenv('DEBUG') == 0){
                self::display('simple', 'Something went wrong');
            }else{
                self::display('full', $error);
            }
        }
    }

    /**
     * Display error messages
     *
     * @param string $name		error page name
     * @param string @message	error messages
     *
     * @static
     * @access public
     * @since Method available since Release 0.1.0
     */
    static function display($name, $message = '') : void {
        self::set_header('500', 'Internal Server Error');
        include('errorpage/'. $name .'.hh');
    }

    /**
     * Calculate a precise time difference.
     *
     * @param string $start result of microtime()
     *
     * @return flat difference in seconds, calculated with minimum precision loss
     *
     * @static
     * @see Debugger::exec_time()
     * @access public
     * @since Method available since Release 0.1.0
     */
    static private function microtime_diff($start)
    {
        $duration = microtime(true) - $start;
        $hours = (int)($duration/60/60);
        $minutes = (int)($duration/60)-$hours*60;
        $seconds = $duration-$hours*60*60-$minutes*60;
        return number_format((float)$seconds, 5, '.', '');
    }

    /**
     * Display execution time (start time - finish time) in human readable form
     * (milliseconds).
     *
     *
     * @static
     * @see Debugger::microtime_diff()
     * @access public
     * @since Method available since Release 0.1.0
     */
    static function exec_time(){
        echo ('<span style="display: table; margin: 0 auto;">Request takes '.(self::microtime_diff(SS_START) * 1000 ) . ' milliseconds</span>');
    }

    static function startProfiling(){
        if(self::$profilerStartTime == 0){
            self::$profilerStartTime = microtime(true);
        }

        self::$time_start = microtime(true);
    }

    static function addProfilingData($point_name = '', $point_type = 'others'){
        $profileData =
            [
                'name' => $point_name,
                'time' => ( self::microtime_diff(self::$time_start) * 1000 ),
                'unit' => 'ms',
                'type' => $point_type
            ];

        array_push(self::$profiles, $profileData);

        self::$time_start = microtime(true);

        return $profileData;
    }

    static function endProfiling(){
        $timeIncludingAutoloader = self::microtime_diff(SS_START) * 1000;
        $timeProfiled = self::microtime_diff(self::$profilerStartTime) * 1000;
        $timeMinusAutoloader = $timeIncludingAutoloader - $timeProfiled;

        $profileData =
            [
                'name' => 'Starting Autoloader',
                'time' => ($timeMinusAutoloader),
                'unit' => 'ms',
                'type' => 'system'
            ];

        array_unshift(self::$profiles, $profileData);
        self::$time_start = 0;
        self::$profilerStartTime = 0;

        return
            [
                'Total Time' => ( $timeIncludingAutoloader ),
                'unit' => 'ms',
                'profiles' => self::$profiles,
            ];
    }
}