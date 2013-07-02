/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when the Java Virtual Machine has encountered incorrect index
 * of a class in the application (which should never occur).
 */
public class NoClassDefFoundError extends LinkageError
{
    /**
     * Creates a NoClassDefFoundError.
     */
    public NoClassDefFoundError()
    {
        super();
    }

    /**
     * Creates a NoClassDefFoundError with the detail message.
     */
    public NoClassDefFoundError(String s)
    {
        super(s);
    }
}
