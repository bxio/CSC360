/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an attempt is been made to store the wrong type of object
 * into an array.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class ArrayStoreException extends RuntimeException
{
    /**
     * Creates an ArrayStoreException.
     */
    public ArrayStoreException()
    {
        super();
    }

    /**
     * Creates an ArrayStoreException with the detail message.
     */
    public ArrayStoreException(String s)
    {
        super(s);
    }
}

