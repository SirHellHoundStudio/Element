/*
    GraphEditorView.cpp - This file is part of Element
    Copyright (C) 2016 Kushview, LLC.  All rights reserved.
*/

#include "gui/GuiCommon.h"
#include "gui/GraphEditorView.h"
#include "Globals.h"
#include "session/UnlockStatus.h"

namespace Element
{

GraphEditorView::GraphEditorView()
{
    setName ("GraphEditor");
    addAndMakeVisible (graph);
    setWantsKeyboardFocus (true);
}

GraphEditorView::~GraphEditorView() { }

void GraphEditorView::willBeRemoved()
{
    graph.setNode (Node());
}

bool GraphEditorView::keyPressed (const KeyPress& key, Component* c)
{
    if (key.getKeyCode() == KeyPress::backspaceKey ||
        key.getKeyCode() == KeyPress::deleteKey)
    {
        if (AlertWindow::showOkCancelBox (AlertWindow::QuestionIcon, "Delete ALL selected nodes?",
            "This operation cannot be undone (yet)"))
        {
            graph.deleteSelectedNodes();
        }

        return true;
    }

    return ContentView::keyPressed (key, c);
}

void GraphEditorView::stabilizeContent()
{
    const auto g = getGraph();
    
    if (g.isGraph() && !g.isRootGraph())
    {
        disableIfNotUnlocked();
    }
    else 
    {
        setEnabled (true); 
        setInterceptsMouseClicks (true, true);
    }

    graph.setNode (g);
}

void GraphEditorView::didBecomeActive()
{
    if (!getGraph().isValid() || !getGraph().isGraph())
    {
        if (auto session = ViewHelpers::getSession (this))
            setNode (session->getCurrentGraph());
    }
    else
    {
        stabilizeContent();
    }

    graph.updateComponents();
}

void GraphEditorView::paint (Graphics& g)
{
    g.fillAll (LookAndFeel::contentBackgroundColor);
}

void GraphEditorView::graphDisplayResized (const Rectangle<int> &area)
{
    graph.setBounds (area);
}

void GraphEditorView::graphNodeChanged (const Node& g, const Node&)
{
    stabilizeContent();
}

} /* namespace Element */