/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Base class for all exception and error objects.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class Throwable
{
    private String detailMessage;

    /**
     * Creates a Throwable object.
     */
    public Throwable()
    {
//        fillInStackTrace();
    }

    /**
     * Creates a Throwable with detailed message.
     */
    public Throwable(String message)
    {
//        fillInStackTrace();
        detailMessage = message;
    }

    /**
     * Returns detailed message if available.
     */
    public String getMessage()
    {
        return detailMessage;
    }

    /**
     * Returns detailed message if available.
     */
    public String getLocalizedMessage()
    {
        return getMessage();
    }

    /**
     * Returns detailed message or empty string.
     */
    public String toString()
    {
        String message = getMessage();
        return (message != null) ? message : "";
    }

    public native void fillInStackTrace();
    public native void getStackTrace(byte[] Buffer);

}
