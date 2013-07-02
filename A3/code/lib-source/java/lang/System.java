/************************************************************************
This file is part of java core libraries for the simpleRTJ virtual machine.

This file is covered by the GNU GPL with the following exception:
  As a special exception, the copyright holders of this library give you permission
  to link this library with independent modules to produce an executable, regardless
  of the license terms of these independent modules, and to copy and distribute the
  resulting executable under terms of your choice, provided that you also meet, for
  each linked independent module, the terms and conditions of the license of that
  module. An independent module is a module which is not derived from or based on
  this library. If you modify this library, you may extend this exception to your
  version of the library, but you are not obligated to do so. If you do not wish
  to do so, delete this exception statement from your version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

Created/modified by:
    RTJ Computing
***********************************************************************/

package java.lang;

public final class System
{
    private System()
    {

    }

    public static native void arraycopy(Object src, int src_position,
                                        Object dst, int dst_position,
                                        int length);
    public static native int freeMemory();
    public static native int totalMemory();
    public static native void gc();

    public static int identityHashCode(Object o)
    {
        return o.hashCode();
    }

    /**
     * Returns specified system property. Only properties relevant to underlying
     * hardware might be implemented in native code.
     * Max key length is limited to 64 characters.
     */
    public static String getProperty(String key)
    {
        if (getPropertyLen(key) == 0)
            return null;

        return getProperty(key, "");
    }

    /**
     * Returns specified system property. Only properties relevant to underlying
     * hardware might be implemented in native code.
     * Max key length is limited to 64 characters.
     */
    public static String getProperty(String key, String def)
    {
        int len = getPropertyLen(key);
        if (len == 0)
            return def;
        byte[] val = new byte[len];
        getProperty0(key, val);

        return new String(val, 0, len);
    }

    /**
     * Returns number of millisecond since VM has been started running. Counter will
     * wrap arround after approximately 298 days.
     */
    public static native int currentTimeMillis();

    /** Prints the specified string to the console **/
    public static native void print(String str);
    public static native void println(String str);

    /** Reads input from the console **/
    public static native String readln();
    
    /** Retrieves the length of specified property. Zero if it doesn't exists. */
    private static native int getPropertyLen(String key);
    /** Copy specified property into destination array. */
    private static native boolean getProperty0(String key, byte[] dest);
}
