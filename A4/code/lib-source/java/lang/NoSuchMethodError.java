/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when the Java Virtual Machine has encountered incorrect index
 * of a method in the application.
 * This may occur when for some reason native method can't be found.
 */
public class NoSuchMethodError extends IncompatibleClassChangeError
{
    /*
     * Creates a NoSuchMethodException.
     */
    public NoSuchMethodError()
    {
        super();
    }

    /**
     * Creates a NoSuchMethodException with the detail message.
     */
    public NoSuchMethodError(String s)
    {
        super(s);
    }
}
