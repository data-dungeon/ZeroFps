struct CtrlInfo
{
	char name[50];
	int id;
};

#define ID_CLOSE			100
#define ID_OPEN_TEMPLATE	101
#define ID_LOAD_MAP			102
#define IDM_CLOSE			203

static CtrlInfo g_kCtrList[] =
{
	{ "Close",			ID_CLOSE			},
	{ "Open template",	ID_OPEN_TEMPLATE	},
	{ "Load map",		ID_LOAD_MAP			},
};
