/*
 * Copyright (c) 1992 The Regents of the University of California
 * Copyright (c) 1990, 1992 Jan-Simon Pendry
 * All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
 * Jan-Simon Pendry.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)umap_vfsops.c	1.1 (Berkeley) 07/11/92
 *
 * @(#)lofs_vfsops.c	1.2 (Berkeley) 6/18/92
 * $Id: lofs_vfsops.c,v 1.9 1992/05/30 10:26:24 jsp Exp jsp $
 */

/*
 * Null Layer
 * (See umap_vnops.c for a description of what this does.)
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/malloc.h>
#include <umapfs/umap.h>

/*
 * Mount umap layer
 */
int
umapfs_mount(mp, path, data, ndp, p)
	struct mount *mp;
	char *path;
	caddr_t data;
	struct nameidata *ndp;
	struct proc *p;
{
	int i;
	int error = 0;
	struct umap_args args;
	struct vnode *lowerrootvp, *vp;
	struct vnode *umapm_rootvp;
	struct umap_mount *amp;
	u_int size;

#ifdef UMAPFS_DIAGNOSTIC
	printf("umapfs_mount(mp = %x)\n", mp);
#endif

	/*
	 * Update is a no-op
	 */
	if (mp->mnt_flag & MNT_UPDATE) {
		return (EOPNOTSUPP);
		/* return VFS_MOUNT(MOUNTTOUMAPMOUNT(mp)->umapm_vfs, path, data, ndp, p);*/
	}

	/*
	 * Get argument
	 */
	if (error = copyin(data, (caddr_t)&args, sizeof(struct umap_args)))
		return (error);

	/*
	 * Find target node
	 */
	NDINIT(ndp, LOOKUP, FOLLOW|WANTPARENT|LOCKLEAF,
		UIO_USERSPACE, args.target, p);
	if (error = namei(ndp))
		return (error);

	/*
	 * Sanity check on target vnode
	 */
	lowerrootvp = ndp->ni_vp;
#ifdef UMAPFS_DIAGNOSTIC
	printf("vp = %x, check for VDIR...\n", lowerrootvp);
#endif
	vrele(ndp->ni_dvp);
	ndp->ni_dvp = 0;

	/*
	 * NEEDSWORK: Is this really bad, or just not
	 * the way it's been?
	 */
	if (lowerrootvp->v_type != VDIR) {
		vput(lowerrootvp);
		return (EINVAL);
	}

#ifdef UMAPFS_DIAGNOSTIC
	printf("mp = %x\n", mp);
#endif

	amp = (struct umap_mount *) malloc(sizeof(struct umap_mount),
				M_UFSMNT, M_WAITOK);	/* XXX */

	/*
	 * Save reference to underlying target FS
	 */
	amp->umapm_vfs = lowerrootvp->v_mount;

	/* 
	 * Now copy in the number of entries and maps for umap mapping.
	 */

	amp->info_nentries = args.nentries;
	amp->info_gnentries = args.gnentries;
	error = copyin(args.mapdata, (caddr_t)amp->info_mapdata, 
	    8*args.nentries);
	if (error) return (error);
	printf("umap_mount:nentries %d\n",args.nentries);
	for (i=0; i<args.nentries;i++)
		printf("   %d maps to %d\n", amp->info_mapdata[i][0],
	 	    amp->info_mapdata[i][1]);

	error = copyin(args.gmapdata, (caddr_t)amp->info_gmapdata, 
	    8*args.nentries);
	if (error) return (error);
	printf("umap_mount:gnentries %d\n",args.gnentries);
	for (i=0; i<args.gnentries;i++)
		printf("	group %d maps to %d\n", 
		    amp->info_gmapdata[i][0],
	 	    amp->info_gmapdata[i][1]);


	/*
	 * Save reference.  Each mount also holds
	 * a reference on the root vnode.
	 */
	error = umap_node_create(mp, lowerrootvp, &vp);
	/*
	 * Unlock the node (either the target or the alias)
	 */
	VOP_UNLOCK(vp);
	/*
	 * Make sure the node alias worked
	 */
	if (error) {
		vrele(lowerrootvp);
		free(amp, M_UFSMNT);	/* XXX */
		return (error);
	}

	/*
	 * Keep a held reference to the root vnode.
	 * It is vrele'd in umapfs_unmount.
	 */
	umapm_rootvp = vp;
	umapm_rootvp->v_flag |= VROOT;
	amp->umapm_rootvp = umapm_rootvp;
	if (UMAPVPTOLOWERVP(umapm_rootvp)->v_mount->mnt_flag & MNT_LOCAL)
		mp->mnt_flag |= MNT_LOCAL;
	mp->mnt_data = (qaddr_t) amp;
	getnewfsid(mp, MOUNT_LOFS);

	(void) copyinstr(path, mp->mnt_stat.f_mntonname, MNAMELEN - 1, &size);
	bzero(mp->mnt_stat.f_mntonname + size, MNAMELEN - size);
	(void) copyinstr(args.target, mp->mnt_stat.f_mntfromname, MNAMELEN - 1, 
	    &size);
	bzero(mp->mnt_stat.f_mntfromname + size, MNAMELEN - size);
#ifdef UMAPFS_DIAGNOSTIC
	printf("umapfs_mount: target %s, alias at %s\n",
		mp->mnt_stat.f_mntfromname, mp->mnt_stat.f_mntonname);
#endif
	return (0);
}

/*
 * VFS start.  Nothing needed here - the start routine
 * on the underlying filesystem will have been called
 * when that filesystem was mounted.
 */
int
umapfs_start(mp, flags, p)
	struct mount *mp;
	int flags;
	struct proc *p;
{
	return (0);
	/* return VFS_START(MOUNTTOUMAPMOUNT(mp)->umapm_vfs, flags, p); */
}

/*
 * Free reference to umap layer
 */
int
umapfs_unmount(mp, mntflags, p)
	struct mount *mp;
	int mntflags;
	struct proc *p;
{
	struct vnode *umapm_rootvp = MOUNTTOUMAPMOUNT(mp)->umapm_rootvp;
	int error;
	int flags = 0;
	extern int doforce;

#ifdef UMAPFS_DIAGNOSTIC
	printf("umapfs_unmount(mp = %x)\n", mp);
#endif

	if (mntflags & MNT_FORCE) {
		/* lofs can never be rootfs so don't check for it */
		if (!doforce)
			return (EINVAL);
		flags |= FORCECLOSE;
	}

	/*
	 * Clear out buffer cache.  I don't think we
	 * ever get anything cached at this level at the
	 * moment, but who knows...
	 */
#if 0
	mntflushbuf(mp, 0); 
	if (mntinvalbuf(mp, 1))
		return (EBUSY);
#endif
	if (umapm_rootvp->v_usecount > 1)
		return (EBUSY);
	if (error = vflush(mp, umapm_rootvp, flags))
		return (error);

#ifdef UMAPFS_DIAGNOSTIC
	/*
	 * Flush any remaining vnode references
	 */
	umap_node_flushmp (mp);
#endif

#ifdef UMAPFS_DIAGNOSTIC
	vprint("alias root of target", umapm_rootvp);
#endif	 
	/*
	 * Release reference on underlying root vnode
	 */
	vrele(umapm_rootvp);
	/*
	 * And blow it away for future re-use
	 */
	vgone(umapm_rootvp);
	/*
	 * Finally, throw away the umap_mount structure
	 */
	free(mp->mnt_data, M_UFSMNT);	/* XXX */
	mp->mnt_data = 0;
	return 0;
}

int
umapfs_root(mp, vpp)
	struct mount *mp;
	struct vnode **vpp;
{
	struct vnode *vp;

#ifdef UMAPFS_DIAGNOSTIC
	printf("umapfs_root(mp = %x, vp = %x->%x)\n", mp,
			MOUNTTOUMAPMOUNT(mp)->umapm_rootvp,
			UMAPVPTOLOWERVP(MOUNTTOUMAPMOUNT(mp)->umapm_rootvp)
			);
#endif

	/*
	 * Return locked reference to root.
	 */
	vp = MOUNTTOUMAPMOUNT(mp)->umapm_rootvp;
	VREF(vp);
	VOP_LOCK(vp);
	*vpp = vp;
	return 0;
}

int
umapfs_quotactl(mp, cmd, uid, arg, p)
	struct mount *mp;
	int cmd;
	uid_t uid;
	caddr_t arg;
	struct proc *p;
{
	return VFS_QUOTACTL(MOUNTTOUMAPMOUNT(mp)->umapm_vfs, cmd, uid, arg, p);
}

int
umapfs_statfs(mp, sbp, p)
	struct mount *mp;
	struct statfs *sbp;
	struct proc *p;
{
	int error;
	struct statfs mstat;

#ifdef UMAPFS_DIAGNOSTIC
	printf("umapfs_statfs(mp = %x, vp = %x->%x)\n", mp,
			MOUNTTOUMAPMOUNT(mp)->umapm_rootvp,
			UMAPVPTOLOWERVP(MOUNTTOUMAPMOUNT(mp)->umapm_rootvp)
			);
#endif

	bzero(&mstat, sizeof(mstat));

	error = VFS_STATFS(MOUNTTOUMAPMOUNT(mp)->umapm_vfs, &mstat, p);
	if (error)
		return (error);

	/* now copy across the "interesting" information and fake the rest */
	sbp->f_type = mstat.f_type;
	sbp->f_flags = mstat.f_flags;
	sbp->f_bsize = mstat.f_bsize;
	sbp->f_iosize = mstat.f_iosize;
	sbp->f_blocks = mstat.f_blocks;
	sbp->f_bfree = mstat.f_bfree;
	sbp->f_bavail = mstat.f_bavail;
	sbp->f_files = mstat.f_files;
	sbp->f_ffree = mstat.f_ffree;
	if (sbp != &mp->mnt_stat) {
		bcopy(&mp->mnt_stat.f_fsid, &sbp->f_fsid, sizeof(sbp->f_fsid));
		bcopy(mp->mnt_stat.f_mntonname, sbp->f_mntonname, MNAMELEN);
		bcopy(mp->mnt_stat.f_mntfromname, sbp->f_mntfromname, MNAMELEN);
	}
	return (0);
}

int
umapfs_sync(mp, waitfor, cred, p)
	struct mount *mp;
	int waitfor;
	struct ucred *cred;
	struct proc *p;
{
	/*
	 * XXX - Assumes no data cached at umap layer.
	 */
	return (0);
}

int
umapfs_vget(mp, ino, vpp)
	struct mount *mp;
	ino_t ino;
	struct vnode **vpp;
{
	
	return VFS_VGET(MOUNTTOUMAPMOUNT(mp)->umapm_vfs, ino, vpp);
}

int
umapfs_fhtovp(mp, fidp, nam, vpp, exflagsp, credanonp)
	struct mount *mp;
	struct fid *fidp;
	struct mbuf *nam;
	struct vnode **vpp;
	int *exflagsp;
	struct ucred**credanonp;
{

	return VFS_FHTOVP(MOUNTTOUMAPMOUNT(mp)->umapm_vfs, fidp, nam, vpp, exflagsp,credanonp);
}

int
umapfs_vptofh(vp, fhp)
	struct vnode *vp;
	struct fid *fhp;
{
	return VFS_VPTOFH(UMAPVPTOLOWERVP(vp), fhp);
}

int umapfs_init __P((void));

struct vfsops umap_vfsops = {
	umapfs_mount,
	umapfs_start,
	umapfs_unmount,
	umapfs_root,
	umapfs_quotactl,
	umapfs_statfs,
	umapfs_sync,
	umapfs_vget,
	umapfs_fhtovp,
	umapfs_vptofh,
	umapfs_init,
};
