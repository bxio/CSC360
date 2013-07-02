/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Thrown when current thread is not owner of the object's monitor.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class IllegalMonitorStateException extends RuntimeException
{
    /**
     * Creates a IllegalMonitorStateException object.
     */
    public IllegalMonitorStateException()
    {
        super();
    }

    /**
     * Creates a IllegalMonitorStateException with the detailed message.
     */
    public IllegalMonitorStateException(String s)
    {
        super(s);
    }
}
