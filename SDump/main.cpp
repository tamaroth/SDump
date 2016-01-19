#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

// Column titles for the list
static const char *const header[] = {
	"Segment name",
	"Start address",
	"End address",
	"Size"
};
const char *popup_names[] = {
	"Insert",					// not active
	"Delete",					// not active
	"Dump segment to disk",		// menu entry for dumping the segment
	"Refresh"					// not active
};
// Window title
static const char *title = "SDump plugin by tamaroth";
// Column widths, CHCOL_HEX flag sets the display of numbers as hexadecimal.
static const int widths[] = { CHCOL_HEX | 8, 20, 20, 20 };

// Retrieve an array of pointers to structures containing segment information.
segment_t** get_seg_data()
{
	int segm_count = get_segm_qty();
	segment_t **segm_ret = (segment_t**)qalloc(segm_count * sizeof(segment_t*));
	if (segm_ret == NULL)
		return NULL;

	for (int i = 0; i < segm_count; i++)
	{
		segm_ret[i] = getnseg(i);
	}

	return segm_ret;
}

// Save a segment to a file on disk.
bool dump_segm(ulong n)
{
	// Get segment.
	segment_t *segm = getnseg(n);
	char *ans;
	char segm_name[MAXSTR];

	// Get the name of the destination file.
	get_segm_name(segm, segm_name, MAXSTR);
	ans = askfile_cv(1, segm_name, "Enter a name of the file.", 0);
	if (!ans)
		return false;

	uint32 segm_size = segm->endEA - segm->startEA;
	uchar *mem = (uchar *)qalloc(segm_size * sizeof(uchar) + 1);
	if (mem == NULL)
		return false;

	// Read bytes, no return check due to function returning 0 if the read bytes contains uninitialized data, which happens very often in case of entire segments.
	get_many_bytes(segm->startEA, mem, segm_size);
	FILE *fp = qfopen(ans, "wb");

	if (fp == NULL)
	{
		qfree(mem);
		return false;
	}

	// Save the data to a file.
	if (qfwrite(fp, mem, segm_size) < 0)
	{
		qfclose(fp);
		qfree(mem);
		return false;
	}

	qfclose(fp);
	qfree(mem);
	return true;
}

// Callback function returning number of rows.
static uint32 idaapi sizer(void *obj)
{
	(void)(obj);
	return get_segm_qty();
}

// Callback function filling the list with data.
void idaapi getl(void *obj, uint32 n, char * const *arrptr)
{
	if (n == 0)
	{
		for (int i = 0; i < qnumber(header); i++)
			qstrncpy(arrptr[i], header[i], MAXSTR);
		return;
	}

	segment_t **segm = (segment_t **)obj;
	get_segm_name(segm[n - 1], arrptr[0], MAXSTR);
	qsnprintf(arrptr[1], MAXSTR, "%08a", segm[n - 1]->startEA);
	qsnprintf(arrptr[2], MAXSTR, "%08a", segm[n - 1]->endEA);
	qsnprintf(arrptr[3], MAXSTR, "%08a", segm[n - 1]->endEA - segm[n - 1]->startEA);
}

// Callback function invoking dumping process by pressing Enter.
static void idaapi enter_cb(void *obj, uint32 n)
{
	segment_t **segm = (segment_t **)obj;
	char segm_name[MAXSTR];
	get_segm_name(segm[n - 1], segm_name, MAXSTR);
	msg("Dumping %s to disk... ", segm_name);
	msg("%s\n", dump_segm(n - 1) ? "Done!" : "Failed!");
	return;
}

// Callback function invoked upon window closure.
static void idaapi destroy_cb(void *obj)
{
	(void)(obj);
	return;
}

// Callback function invoking dumping process by using pop-up menu, identical to enter_cb
static void idaapi edit_cb(void *obj, uint32 n)
{
	segment_t **segm = (segment_t **)obj;
	char segm_name[MAXSTR];
	get_segm_name(segm[n - 1], segm_name, MAXSTR);
	msg("Dumping %s to disk... ", segm_name);
	msg("%s\n", dump_segm(n - 1) ? "Done!" : "Failed!");
	return;
}

// Function executed upon initialization of the plugin.
int idaapi init(void)
{
	msg("SDump plugin loaded ...\n");
	return PLUGIN_OK;
}

// Function executed upon termination of the plugin
void idaapi term(void)
{
	return;
}

// Function executed when the plugin is ran.
void idaapi run(int arg)
{
	(void)(arg);
	// Retrieve information about segments
	segment_t **info = get_seg_data();
	if (info == NULL)
	{
		warning("SDump: Error while allocating memory...\n");
		return;
	}

	// Show a list of segments.
	auto res = choose2(
		info,				// object
		qnumber(header),	// number of columns
		widths,				// widths of columns
		sizer,				// get number of rows
		getl,				// get current line
		title,				// title
		-1,					// default icon
		(uint32)-1,
		NULL,				// delete callback
		NULL,				// insert callback
		NULL,				// update callback
		edit_cb,			// edit callback
		enter_cb,			// enter callback
		destroy_cb,			// destroy callback,
		popup_names,		// default pop-up names
		NULL				// default icon
		);

	if (res > 0)
	{
		char segm_name[MAXSTR];
		get_segm_name(info[res - 1], segm_name, MAXSTR);
		msg("Dumping %s to disk... ", segm_name);
		msg("%s\n", dump_segm(res - 1) ? "Done!" : "Failed!");
	}
	qfree(info);
	return;
}

/************************************************************************/
/* Plugin information and initialization.                               */
/************************************************************************/

static const char comment[] = "This plugin allows you to dump segments to disk.";
static const char help[] = "With this plugin you can dump segments to disk while debugging a program. To activate, use Alt-5";
static const char wanted_name[] = "SDump";
static const char wanted_hotkey[] = "Alt-5";

plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	0,						// plugin flags
	init,					// initialize
	term,					// terminate
	run,					// invoke
	comment,				// comment
	help,					// help about plugin
	wanted_name,			// name of the plugin
	wanted_hotkey			// hotkey to run the plugin
};
