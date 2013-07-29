/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an exceptional arithmetic condition has occurred.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class ArithmeticException extends RuntimeException
{
    /**
     * Creates an ArithmeticException.
     */
    public ArithmeticException()
    {
        super();
    }

    /**
     * Creates an ArithmeticException with detail message.
     */
    public ArithmeticException(String s)
    {
        super(s);
    }
}
