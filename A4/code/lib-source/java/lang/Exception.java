/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 *
 * For detailed description of this class see the Java language documentation.
 */
public class Exception extends Throwable
{
    /**
     * Creates an Exception class.
     */
    public Exception()
    {
        super();
    }

    /**
     * Creates an Exception with the detail message.
     */
    public Exception(String s)
    {
        super(s);
    }
}
