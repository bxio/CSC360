/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown if a stack overflow occurs.
 */
public class StackOverflowError extends VirtualMachineError
{
    /**
     * Creates StackOverflowError object.
     */
    public StackOverflowError()
    {
        super();
    }

    /**
     * Creates a StackOverflowError with the detail message.
     */
    public StackOverflowError(String s)
    {
        super(s);
    }
}
