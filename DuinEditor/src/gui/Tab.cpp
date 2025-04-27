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
            { guitag::TAB_UUID, duin::UUID::ToStringHex(uuid)},
            { guitag::TAB_PANELMANAGER_UUID, GetPanelManagerID()}
        });
    
    panelManager.Serialise();

    return value;
}

void Tab::Deserialise(duin::TOMLValue value)
{
    title = value.At(guitag::TAB_TITLE).AsString();
    uuid = value.At(guitag::TAB_UUID).AsInteger();
    panelManager = PanelManager(value);
}
