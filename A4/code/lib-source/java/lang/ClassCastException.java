/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an attempted is made to cast an object to an inappropriate subclass.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class ClassCastException extends RuntimeException
{
    /**
     * Creates a ClassCastException.
     */
    public ClassCastException()
    {
        super();
    }

    /**
     * Creates a ClassCastException with the detail message.
     */
    public ClassCastException(String s)
    {
        super(s);
    }
}
