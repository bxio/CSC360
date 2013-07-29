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
 * Java VM generates an IndexOutOfBoundsException.
 */
public class ArrayIndexOutOfBoundsException extends IndexOutOfBoundsException
{
    public ArrayIndexOutOfBoundsException()
    {
        super();
    }

    public ArrayIndexOutOfBoundsException(int index)
    {
        super("Array index out of range: " + index);
    }

    public ArrayIndexOutOfBoundsException(String s)
    {
        super(s);
    }
}
