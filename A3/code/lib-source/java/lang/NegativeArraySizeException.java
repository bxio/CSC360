/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when an application tries to create an array with negative size.
 */
public class NegativeArraySizeException extends RuntimeException
{
    /**
     * Creates a NegativeArraySizeException class.
     */
    public NegativeArraySizeException()
    {
        super();
    }

    /**
     * Creates a NegativeArraySizeException with the detail message.
     */
    public NegativeArraySizeException(String s)
    {
        super(s);
    }
}
