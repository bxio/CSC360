/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Thrown when parsed string doesn't contain expected characters.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class NumberFormatException extends IllegalArgumentException
{
    public NumberFormatException ()
    {
        super();
    }

    public NumberFormatException (String s)
    {
        super(s);
    }
}
