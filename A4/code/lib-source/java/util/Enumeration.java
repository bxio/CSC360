/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.util;

public interface Enumeration
{
  public abstract boolean hasMoreElements();
  public abstract Object nextElement();
}
