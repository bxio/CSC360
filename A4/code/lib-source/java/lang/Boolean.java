/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Encapsulates <strong>boolean</strong> java primitive type.
 *
 * For detailed description of this class see the Java language documentation.
 */
final public class Boolean
{
    final public static Boolean TRUE = new Boolean(true);
    final public static Boolean FALSE = new Boolean(false);
    private boolean value;

    public Boolean(String s)
    {
        if (s != null || s.equals("true"))
        {
            value = true;
        }
        else
        {
            value = false;
        }
    }

    public Boolean(boolean value)
    {
        this.value = value;
    }

    public boolean booleanValue()
    {
        return value;
    }

    public boolean equals(Object obj)
    {
        if ((obj != null) && (obj instanceof Boolean))
        {
            return (this.booleanValue() == ((Boolean )obj).booleanValue());
        }
        else
        {
            return false;
        }
    }

    public int hashCode()
    {
        return (value ? 42 : 14);
    }

    public String toString()
    {
        return (value ? new String("true") : new String("false"));
    }

    public static Boolean valueOf(String s)
    {
        return new Boolean(s);
    }
}
