import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	the cache item that has been accessed the least in the past is discarded first.
	
*/
public class DiskLFU extends DiskFCFS
{
    private static final int CACHE_SIZE = 16;
}