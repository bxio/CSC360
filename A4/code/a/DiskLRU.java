import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	the least recently used cache item is discarded first.

*/
public class DiskLRU extends DiskFCFS
{
    private static final int CACHE_SIZE = 16;

}