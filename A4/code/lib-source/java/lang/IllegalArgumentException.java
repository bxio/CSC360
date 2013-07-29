/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Thrown when a method has been passed an illegal or inappropriate argument.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class IllegalArgumentException extends RuntimeException
{
    public IllegalArgumentException()
    {
        super();
    }

    public IllegalArgumentException(String s)
    {
        super(s);
    }
}
