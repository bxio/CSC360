/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an index to a String object is out of bounds.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class StringIndexOutOfBoundsException extends RuntimeException
{
    /**
     * Creates a StringIndexOutOfBoundsException object.
     */
    public StringIndexOutOfBoundsException()
    {
        super();
    }

    /**
     * Creates a StringIndexOutOfBoundsException with the detailed message.
     */
    public StringIndexOutOfBoundsException(String s)
    {
        super(s);
    }

    /**
     * Creates a StringIndexOutOfBoundsException with offending index.
     */
    public StringIndexOutOfBoundsException(int index)
    {
        super("String index out of range: " + index);
    }
}
