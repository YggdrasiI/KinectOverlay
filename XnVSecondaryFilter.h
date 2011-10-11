/*
 From Lior Cohen
 See http://groups.google.com/group/openni-dev/browse_frm/thread/7cb51f1fa3240111/19b7ceb3ad1cc997?#19b7ceb3ad1cc997
*/

#ifndef XNV_SECONDARY_FILTER_H
#define XNV_SECONDARY_FILTER_H

// Secondary filter.
// This class replaces the primary point with another. The other point will remain primary for all its subtree
// until it is either removed or becomes the real primary. In these cases another point will become the primary
// for the subtree.
// Notice: If there is only one point in the system, it will be the primary for both the original tree and
// the filter's subtree, until another point is recognized. The filter will then replace the primary for
// its subtree.
class XnVSecondaryFilter : public XnVPointFilter
{
public:
	// Constructor.
	XnVSecondaryFilter() :
	  m_nNewPrimary(0)
	  {}

	// Create a local copy of all the hands, and set a primary for the subtree.
	void Update(const XnVMultipleHands& hands)
	{
		//printf("Update (MultipleHands)\n");

		// Copy all the hands
		hands.Clone(m_LocalCopy);

		const XnVHandPointContext* pPrimary = hands.GetPrimaryContext();
		if (pPrimary == NULL)
		{
			// There are no points at all.
			return;
		}

		/*XnUInt32*/ nPrimary = pPrimary->nID;

		if (hands.GetContext(m_nNewPrimary) == 0)
		{
			// The secondary point we considered primary for the subtree is no longer available.
			m_nNewPrimary = 0;
		}

		// If we don't remember a secondary point as primary, or that secondary just became the primary for 
		// the entire tree, try to locate another secondary.
		if (m_nNewPrimary == 0 || m_nNewPrimary == nPrimary)
		{
			// local primary unavailable
			for (XnVMultipleHands::ConstIterator iter = hands.begin(); iter != hands.end(); ++iter)
			{
				if (iter.IsActive() && (*iter)->nID != nPrimary)
				{
					// Found a point that is active, but is not the primary. This will be the primary
					// for the subtree
					m_nNewPrimary = (*iter)->nID;
					break;
				}
			}
		}
		// Adjust the hand list with our chosen primary for the subtree
		m_LocalCopy.ReassignPrimary(m_nNewPrimary);
	}

	// Received a new message
	void Update(XnVMessage* pMessage)
	{
		//printf("Update (pMessage)\n");//wird immer nach obiger Update-Funktion aufgerufen.

		// Deal with point message (calls Update(const XnVMultipleHands&)). This will fill the local copy.
		XnVPointControl::Update(pMessage);

		// Replace the points part of the message with the local copy.
		GenerateReplaced(pMessage, m_LocalCopy);

		// If there are no active entries (which means - no points), a new point will be looked for next time.
		if (m_LocalCopy.ActiveEntries() == 0)
		{
			m_nNewPrimary = 0;
		}
	}

  // Set Primary node  manually
	void setPrimary(XnUInt32 nP){
		nPrimary = nP;
	}

protected:
	XnUInt32 m_nNewPrimary;
	XnVMultipleHands m_LocalCopy;
public:
		XnUInt32 nPrimary;
};

#endif
