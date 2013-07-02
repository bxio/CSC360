/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an index is out of range.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class IndexOutOfBoundsException extends RuntimeException
{
    /**
     * Creates an IndexOutOfBoundsException.
     */
    public IndexOutOfBoundsException()
    {
        super();
    }

    /**
     * Creates an IndexOutOfBoundsException with the detail message.
     */
    public IndexOutOfBoundsException(String s)
    {
        super(s);
    }
}
