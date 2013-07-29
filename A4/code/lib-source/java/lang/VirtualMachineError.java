/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * This class is not used by the Java VM. Is here because it is a predefined class.
 *
 * Java VM generates an InternalError with corresponding detail code.
 */
abstract public class VirtualMachineError extends Error
{
    public VirtualMachineError()
    {
        super();
    }

    public VirtualMachineError(String s)
    {
        super(s);
    }
}
