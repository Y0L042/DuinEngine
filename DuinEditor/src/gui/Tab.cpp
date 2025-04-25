#include "Tab.h"
#include "GuiMeta.h"

Tab::Tab(duin::TOMLValue value)
{
    Deserialise(value);
}

std::string Tab::GetPanelManagerID()
{
    return duin::UUID::ToStringHex(panelManager.GetUUID());
}

duin::UUID Tab::GetUUID()
{
    return uuid;
}

duin::TOMLValue Tab::Serialise()
{
    duin::TOMLValue value({
            { guitag::TAB_TITLE, title },
            { guitag::TAB_ID, id },
            { guitag::TAB_PANELMANAGER, GetPanelManagerID()}
        });
    
    panelManager.Serialise();

    return value;
}

void Tab::Deserialise(duin::TOMLValue value)
{
    title = value.At(guitag::TAB_TITLE).AsString();
    id = value.At(guitag::TAB_ID).AsInteger();
    panelManager = PanelManager(value);
}
