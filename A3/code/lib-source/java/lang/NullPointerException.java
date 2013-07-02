/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when Java VM encounters a null reference to an object.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class NullPointerException extends RuntimeException
{
    /**
     * Creates a NullPointerException.
     */
    public NullPointerException()
    {
        super();
    }

    /**
     * Creates a NullPointerException with the detail message.
     */
    public NullPointerException(String s)
    {
        super(s);
    }
}
