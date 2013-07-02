/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when Java VM can't find native method that matchech one in the
 * native method lookup table.
 */
public class VerifyError extends LinkageError
{
    /**
     * Creates a VerifyError object.
     */
    public VerifyError()
    {
        super();
    }

    /**
     * Creates a VerifyError with the detail message.
     */
    public VerifyError(String s)
    {
        super(s);
    }
}
