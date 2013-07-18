/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when the Java Virtual Machine has encountered incorrect index
 * of a field in the application (which should never occur).
 */
public class NoSuchFieldError extends IncompatibleClassChangeError
{
    /**
     * Creates a NoSuchFieldException.
     */
    public NoSuchFieldError()
    {
        super();
    }

    /**
     * Creates a NoSuchFieldException with the detail message.
     */
    public NoSuchFieldError(String s)
    {
        super(s);
    }
}
