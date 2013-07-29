/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an application tries to call an abstract method.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class AbstractMethodError extends IncompatibleClassChangeError
{
    /**
     * Constructs an AbstractMethodError.
     */
    public AbstractMethodError()
    {
        super();
    }

    /**
     * Constructs an AbstractMethodError with detail message.
     */
    public AbstractMethodError(String s)
    {
        super(s);
    }
}
